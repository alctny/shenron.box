use std::env::args;
use std::fs;
use std::path::Path;

fn main() {
    let arg: Vec<String> = args().collect();
    let mut path = ".";
    if arg.len() > 1 {
        path = &arg[1];
    }
    let mut pam: &str = " ";
    if arg.len() > 2 {
        pam = &arg[2];
    }

    rsfn(path, pam);
}

fn rsfn(p: &str, pam: &str) {
    let dir = fs::read_dir(p).expect("open dir error");
    for f in dir
        .filter_map(Result::ok)
        .filter(|f| f.file_type().is_ok() && !f.file_name().to_string_lossy().starts_with("."))
    {
        match f.file_type().unwrap().is_dir() {
            true => {
                let f = &f.file_name().to_string_lossy().to_string();
                let nf = term(f).replace(pam, "");
                let op = Path::new(p).join(f).to_string_lossy().to_string();
                let np = Path::new(p).join(nf).to_string_lossy().to_string();
                fs::rename(op, &np).unwrap();
                rsfn(&np, pam);
            }
            false => {
                let f = &f.file_name().to_string_lossy().to_string();
                let nf = term(f).replace(pam, "");
                let op = Path::new(p).join(f).to_string_lossy().to_string();
                let np = Path::new(p).join(nf).to_string_lossy().to_string();
                fs::rename(op, &np).unwrap();
            }
        };
    }
}

fn term(s: &str) -> String {
    let term = vec![
        '，', '。', '【', '】', '（', '）', '(', ')', ' ', '[', ']', ':', '：', '「', '」', '\'',
        '"', '|', '<', '>', '《', '》', ']', '[', ';', '；', '?', '！',
    ];
    let mut res = String::from(s);
    for v in term {
        res = res.replace(v, "");
    }
    res
}
