use std::env::args;
use std::fs;
use std::path::Path;

fn main() {
    let arg: Vec<String> = args().collect();
    let path: &str;
    let pam: &str;
    match arg.len() {
        1 => (path, pam) = (".", ""),
        2 => (path, pam) = (".", &arg[1]),
        3 => (path, pam) = (&arg[1], &arg[2]),
        _ => {
            eprintln!("too args");
            return;
        }
    };
    rsfn(path, pam);
}

fn rsfn(p: &str, pam: &str) {
    let dir = fs::read_dir(p).expect("open dir error");
    for file in dir
        .filter_map(Result::ok)
        .filter(|f| f.file_type().is_ok() && !f.file_name().to_string_lossy().starts_with("."))
    {
        match file.file_type().unwrap().is_dir() {
            true => {
                let file_name = &file.file_name().to_string_lossy().to_string();
                let new_file_name = term(&file_name.replace(pam, "")).to_lowercase();
                let old_path = Path::new(p).join(file_name).to_string_lossy().to_string();
                let new_path = Path::new(p)
                    .join(new_file_name)
                    .to_string_lossy()
                    .to_string();
                fs::rename(old_path, &new_path).unwrap();
                rsfn(&new_path, pam);
            }
            false => {
                let file_name = &file.file_name().to_string_lossy().to_string();
                let new_file_name = term(&file_name.replace(pam, "")).to_lowercase();
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
        '（', '）', '，', '；', '！', '：', '。', '【', '】', '(', ')', ' ', '[', ']', ':', '「',
        '」', '\'', '"', '|', '<', '>', '《', '》', ']', '[', ';', '?',
    ];
    let mut res = String::from(s);
    for v in term {
        res = res.replace(v, "");
    }
    res
}
