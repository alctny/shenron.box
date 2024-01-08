use std::collections::HashMap;
use std::env;
use std::fmt::Display;
use std::fs;
use std::path::Path;

#[derive(Hash, Eq)]
enum StatKind {
    Directory,
    Unknown,
    Suffix(String),
}

impl PartialEq for StatKind {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (StatKind::Directory, StatKind::Directory) => true,
            (StatKind::Unknown, StatKind::Unknown) => true,
            (StatKind::Suffix(s1), StatKind::Suffix(s2)) => s1.eq_ignore_ascii_case(s2),
            _ => false,
        }
    }
}

impl Display for StatKind {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            StatKind::Directory => write!(f, "Directory"),
            StatKind::Suffix(suffix) => write!(f, "{}", suffix),
            StatKind::Unknown => write!(f, "Unknown"),
        }
    }
}

struct Stat {
    kind: String,
    count: u32,
}

// recursion statistics of directory
fn stat_dir(path: &str, st: &mut HashMap<StatKind, u32>) {
    for entry in fs::read_dir(path).unwrap() {
        let path = entry.unwrap().path();
        let kind = match path.is_dir() {
            true => StatKind::Directory,
            false => match file_suffix(path.to_str().unwrap()) {
                Some(suffix) => StatKind::Suffix(suffix),
                None => StatKind::Unknown,
            },
        };

        match kind {
            StatKind::Directory => {
                *st.entry(StatKind::Directory).or_default() += 1;
                stat_dir(path.to_str().unwrap(), st)
            }
            StatKind::Suffix(suffix) => *st.entry(StatKind::Suffix(suffix)).or_default() += 1,
            StatKind::Unknown => *st.entry(StatKind::Unknown).or_default() += 1,
        }
    }
}

// Get the file suffix
fn file_suffix(path: &str) -> Option<String> {
    let file_name = path.split("/").last().unwrap();
    if file_name.contains(".") {
        Some(file_name.split(".").last().unwrap().to_owned())
    } else {
        None
    }
}

// show statistics by count
fn show_stat(st: &HashMap<StatKind, u32>) {
    let mut st: Vec<Stat> = st
        .into_iter()
        .map(|s| Stat {
            kind: s.0.to_string(),
            count: *s.1,
        })
        .collect();
    st.sort_by(|a, b| a.count.cmp(&b.count));
    for s in st {
        println!("{:11}{}", s.kind, s.count);
    }
}

fn main() {
    let mut stat: HashMap<StatKind, u32> = HashMap::new();
    let args: Vec<_> = env::args().collect();
    let path = if args.len() > 1 { &args[1] } else { "." };
    match Path::new(&path).canonicalize().unwrap().to_str() {
        Some(path) => stat_dir(path, &mut stat),
        None => panic!("get absolute path failed"),
    }
    show_stat(&stat);
}
