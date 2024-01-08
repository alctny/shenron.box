use std::{
    fs::{self, File},
    io::{Error, Read},
    path::Path,
};

use chrono::Local;
use dirs;
use rand::{self, Rng};

static LINE: usize = 20;
static CAP: usize = 25;
static DATA_PATH: &str = "document/rngst_data";

fn main() {
    let data = gen_range_matrix(LINE, CAP);
    let data = matrix_to_string(&data);
    // show_matrix(&data);
    match check_file() {
        Ok((path, exists)) => match exists {
            false => {
                fs::write(path, &data).unwrap();
                println!("{}", data);
            }
            true => {
                let mut buf = String::new();
                File::open(path).unwrap().read_to_string(&mut buf).unwrap();
                println!("{}", buf);
            }
        },
        Err(e) => println!("Error: {}", e),
    }
}

fn gen_range_matrix(line: usize, column: usize) -> Vec<Vec<i32>> {
    let mut matrix = vec![vec![0; column]; line];
    for i in 0..line {
        for j in 0..column {
            matrix[i][j] = rand::thread_rng().gen_range(1..100);
        }
    }
    matrix
}

fn matrix_to_string(matrix: &Vec<Vec<i32>>) -> String {
    let mut result = String::new();
    matrix.iter().for_each(|line| {
        let mut s = line
            .iter()
            .map(|x| format!(" {:02}", x))
            .collect::<Vec<String>>()
            .join(" ");
        s.push('\n');
        result.push_str(&s);
    });
    result
}

fn check_file() -> Result<(String, bool), Error> {
    // 路径不存在不算错误，应该创建路径
    let home_dir = dirs::home_dir().unwrap();
    let home_dir = home_dir.to_str().unwrap();
    let data_path = format!("{}/{}", home_dir, DATA_PATH);
    if !Path::new(&data_path).exists() {
        fs::create_dir_all(&data_path)?;
    }
    let file_name = Local::now().format("%Y-%m-%d.txt");
    let path = format!("{}/{}", data_path, file_name);
    let exists = Path::new(&path).exists();
    Ok((path, exists))
}
