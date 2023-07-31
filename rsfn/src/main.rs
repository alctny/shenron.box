use clap::Parser;
use std::fs;
use std::path::Path;

#[derive(Parser)]
#[command(author, version, about, long_about=None)]
struct RsfnArg {
    #[arg(long, default_value_t=String::from("."))]
    path: String,
    #[arg(long, default_value_t=String::from(""))]
    pam: String,
    #[arg(long, default_value_t=String::from(""))]
    to: String,
}

fn main() {
    let arg = RsfnArg::parse();
    rsfn(&(arg.path), &(arg.pam), &(arg.to));
}

fn rsfn(p: &str, pam: &str, to: &str) {
    let dir = fs::read_dir(p).expect("open dir error");
    for file in dir
        .filter_map(Result::ok)
        .filter(|f| f.file_type().is_ok() && !f.file_name().to_string_lossy().starts_with("."))
    {
        match file.file_type().unwrap().is_dir() {
            true => {
                let file_name = &file.file_name().to_string_lossy().to_string();
                let new_file_name = term(&file_name.replace(pam, to));
                let old_path = Path::new(p).join(file_name).to_string_lossy().to_string();
                let new_path = Path::new(p)
                    .join(new_file_name)
                    .to_string_lossy()
                    .to_string();
                fs::rename(old_path, &new_path).unwrap();
                rsfn(&new_path, pam, to);
            }
            false => {
                let file_name = &file.file_name().to_string_lossy().to_string();
                let new_file_name = term(&file_name.replace(pam, to));
                let old_path = Path::new(p).join(file_name).to_string_lossy().to_string();
                let new_path = Path::new(p)
                    .join(new_file_name)
                    .to_string_lossy()
                    .to_string();
                fs::rename(old_path, &new_path).unwrap();
            }
        };
    }
}

fn term(s: &str) -> String {
    let term = vec![
        '（', '）', '，', '；', '！', '：', '、', '。', '【', '】', '(', ')', ' ', '[', ']', ':',
        '「', '」', '\'', '"', '|', '<', '>', '《', '》', ']', '[', ';', '?',
    ];
    let mut res = String::from(s);
    for v in term {
        res = res.replace(v, "");
    }
    res
}
