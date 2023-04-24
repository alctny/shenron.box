use chrono::prelude::*;

fn main() {
    let start = Local::now();
    let (mut d, mut h) = (0, 0);
    for i in 0..=23 {
        match i {
            0..=8 => (d, h) = (start.day(), 9),
            9..=11 => (d, h) = (start.day(), 12),
            12..=13 => (d, h) = (start.day(), 14),
            14..=17 => (d, h) = (start.day(), 18),
            18..=23 => (d, h) = (start.day() + 1, 0),
            _ => unreachable!(),
        }
    }

    let end = Local
        .with_ymd_and_hms(start.year(), start.month(), d, h, 0, 0)
        .unwrap();
    let dura = (end - start).num_seconds();
    let mut residue = dura.abs();
    let h = residue / 3600;
    residue = residue - 3600 * h;
    let m = residue / 60;
    let s = residue - 60 * m;

    match dura > 0 {
        true => println!("{:02}:{:02}:{:02}", h, m, s),
        false => println!("-{:02}:{:02}:{:02}", h, m, s),
    };
}
