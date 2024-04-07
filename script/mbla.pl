use strict;
use warnings;

use File::Spec;
use File::Basename;

$^M = "1024M";

my $VERSION = '0.10.1';    # 版本号
my $usage   = <<EOF;       # 帮助信息
Usage：
    -t <table>    对表名进行筛选，默认不做筛选
    -v <value>    对字段值进行筛选，默认不做筛选
    -o <output>   设置输出路徑
    -f <file>     输入文件，有 --txt 参数则为解码之后的文本文件，否则为二进制日志文件
    -m <max>      指定每个 CSV 最大行数
    --txt         原格式已经解码，直接进入 SQL 提取阶段
    --help        显示本帮助信息
    --version     显示版本信息
EOF

# -d <dir>      指定目录，启用此参数会对目录下所有文件进行处理
#               请确定该目录下的所有文件都是已经解码后的文件或待解码的二进制日志文件，不可混合存放
#               如果同时启用 -o -d 参数，则输出文件名为 <output>_源文件名_txt_编号.csv

my $STATUS_NONE_REX      = qr/#\sat\s\d+.*/;                           # 事务开始标志
my $STATUS_BEGIN_REX     = qr/^#(\d{6}\s+\d{0,2}:\d{0,2}:\d{0,2})/;    # 事务开始时间
my $STATUS_SQL_REX       = qr/###\s(\w*)\s\w*\s*`(\w+)`.`(\w+)`/;      # SQL 语句
my $STATUS_WHERE_SET_REX = qr/###\s(WHERE|SET)/;    # WHERE 或 SET
my $STATUS_PARAMETER_REX = qr/^###\s+@\d+=(.+)/;    # 参数列表

my $out_index = 0;                                  # 输出文件
my $csv       = "";                                 # 输出的 csv 文件
my $out       = undef;                              # 转化后的文本格式文件
my $mlog      = undef;                              # 二进制日志文件

# TODO: 优化参数解析，目前只能做到正确使用情况下的解析，但是对于错误的参数使用情况，无法给出提示
# 且对于没有提供选项参数的时候可能会导致解析结果与预期不符
sub opts {
    my $args = join( " ", @ARGV );
    my %arg  = ();
    $arg{"table"}  = $1 if ( $args =~ m/.*-t\s+(\S+)\s*/ );
    $arg{"value"}  = $1 if ( $args =~ m/.*-v\s+(\S+)\s*/ );
    $arg{"output"} = $1 if ( $args =~ m/.*-o\s+(\S+)\s*/ );
    $arg{"file"}   = $1 if ( $args =~ m/.*-f\s+(\S+)\s*/ );
    $arg{"dir"}    = $1 if ( $args =~ m/.*-d\s+(\S+)\s*/ );
    $arg{"max"}    = $1 if ( $args =~ m/.*-m\s+(\S+)\s*/ );

    $arg{"txt"}     = $1 if ( $args =~ m/(\s*--txt\s*)/ );
    $arg{"help"}    = $1 if ( $args =~ m/(\s*--help\s*)/ );
    $arg{"version"} = $1 if ( $args =~ m/(\s*--version\s*)/ );
    return \%arg;
}

sub time_format {
    my $t = $_[0];
    $t =~ /(\d{2})(\d{2})(\d{2})\s+(\d{1,2}):(\d{2}):(\d{2})/;
    return "20" . $1 . "-" . $2 . "-" . $3 . " " . $4 . ":" . $5 . ":" . $6;
}

my $arg = opts();

if ( $arg->{"help"} ) {
    print $usage;
    exit 0;
}
elsif ( $arg->{"version"} ) {
    print "mbla.pl version " . $VERSION . "\n";
    exit 0;
}

print "options: \n";
foreach ( keys %$arg ) {
    print "\t$_" . ":\t" . $arg->{$_} . "\n" if ( defined $arg->{$_} );
}

if ( !$arg->{"txt"} ) {
    print "Converting binlog to text file in progress.\n";

    # 格式转换
    if ( $arg->{"file"} ) {
        if ( $arg->{"output"} ) {
            my $filename = basename( $arg->{"file"} );
            $mlog = File::Spec->catfile( $arg->{"output"}, $filename . "_raw" );
        }
        else {
            $mlog = $arg->{"file"} . "_raw";
        }
        my $cmd =
"mysqlbinlog --no-defaults --base64-output=decode-rows -v --result-file="
          . $mlog . " "
          . $arg->{"file"};

        if ( system($cmd) ) {
            print( "try: \"" . $cmd . "\" faild\n" );
            exit(1);
        }
    }
    elsif ( $arg->{"dir"} ) {
        print("it's todoing\n");
        exit(1);
    }
    else {
        print
          "-f or -d is required\n you can use --help to get more information\n";
        exit 0;
    }
}
else {
    $mlog = $arg->{"file"};
}
print "starting extract sql from text file.\n";

# TODO 如果是目录，需要对目录下的所有文件进行处理

sub new_out {
    if ($out) {
        close $out;
    }

    $out_index += 1;
    $csv = $mlog . sprintf( "_%09d.csv", $out_index );
    open( $out, ">", $csv ) or die "Can't open $csv: $!";
    print $out "发生时间,表名,操作类型,参数列表\n";
}
new_out();

# CSV 最大行数，超过这个行数会自动创建新的 CSV 文件
# WPS 能处理的 Excel 最大上限为 1,048,576 行
my $max_row = ( $arg->{"max"} or 1_000_000 );

# TODO：将以下代码封装为子程序

open( my $in, "<", $mlog ) or die "Can't open $mlog: $!";

my $status    = -1;
my @row       = ();
my @parameter = ();
my $row_count = 0;
foreach my $line (<$in>) {

    if ( $status == -1 ) {
        if ( $line =~ $STATUS_NONE_REX ) {
            undef @row;
            undef @parameter;
            $status += 1;
        }
    }
    elsif ( $status == 0 ) {

        # 理论上来说在输出到 csv 文件处做判断是最好的，可以避免因为后续数据都被过滤掉而导致的空文件
        # 但考虑到目前输出到 csv 文件的过程还没有封装为函数，不方便做集中处理，所以暂时在这里做判断
        if ( $row_count >= $max_row ) {
            close $out;
            new_out();
            $row_count = 0;
        }

        if ( $line =~ $STATUS_BEGIN_REX ) {
            @row[0] = time_format $1;
            $status += 1;
        }
        else {
            $status = -1;
        }
    }

    # SQL 语句，操作方法，数据库，表名
    elsif ( $status == 1 ) {
        if ( $line =~ $STATUS_SQL_REX ) {
            my ( $action, $dbname, $tablename ) = ( $1, $2, $3 );
            if ( !$arg->{"table"} or $3 =~ /$arg->{"table"}/ ) {
                push @row, $action, $dbname . "." . $tablename;
                $status += 1;
            }
            else {
                $status = -1;
            }
        }
        elsif ( $line =~ $STATUS_NONE_REX ) {
            $status = 0;
        }
        else {
            undef @row;
            $status = -1;
        }
    }

    # 参数列表
    elsif ( $status == 2 ) {

        # 参数列表
        if ( $line =~ $STATUS_PARAMETER_REX ) {
            push @parameter, $1;
        }

        # 一个事务里面包含了多个 SQL 语句
        elsif ( $line =~ $STATUS_SQL_REX ) {
            if ( !$arg->{"value"}
                or grep { $_ =~ /$arg->{"value"}/ } @parameter )
            {
                print $out join( ",", ( @row, @parameter ) ) . "\n";
                @row = ( $row[0] );
                undef @parameter;
                push @row, $1, $2 . "." . $3;
                $row_count += 1;
            }
        }

        # UPDATE 语句记录为两条数据，分别是原数据和修改之后的数据
        elsif ( ( $line =~ $STATUS_WHERE_SET_REX ) and (@parameter) ) {

            # FIXME: 如果一个 UPDATE 语句中只有 SET 参数或 WHERE 参数含有 需要匹配的关键字，只能看到一条数据
            # 所以应当在上一条为 UPDATE 的 WHERE 语句的时候记录其值（有状态的状态机）
            if ( !$arg->{"value"}
                or grep { $_ =~ /$arg->{"value"}/ } @parameter )
            {
                print $out join( ",", ( @row, @parameter ) ) . "\n";
                undef @parameter;
                $row_count += 1;
            }
        }

        # 事务结束
        elsif ( $line =~ $STATUS_NONE_REX ) {
            if ( !$arg->{"value"}
                or grep { $_ =~ /$arg->{"value"}/ } @parameter )
            {
                print $out join( ",", ( @row, @parameter ) ) . "\n";
                undef @row;
                undef @parameter;
                $status = 0;
                $row_count += 1;
            }
        }
    }

    # 结束标志和其他内容
    elsif ( $status == 3 ) {
        $status = -1;
    }
}


print "Done.\n";
