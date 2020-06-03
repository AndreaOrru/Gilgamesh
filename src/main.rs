#![allow(dead_code)]

use clap::clap_app;
use std::io;

mod app;
mod command;
mod rom;

use app::App;
use rom::ROM;

/// Get the ball rolling.
fn main() -> io::Result<()> {
    // Validation function to test the existence of a file.
    let file_exists = |path: &str| {
        if std::fs::metadata(path).is_ok() {
            Ok(())
        } else {
            Err(String::from("File doesn't exist.\n"))
        }
    };

    // Clap's argument parser.
    let matches = clap_app!(
        gilgamesh =>
            (version: "0.0.1")
            (author: "Andrea Orru <andrea@orru.io>")
            (about: "The definitive reverse engineering toolkit for SNES.")
            (@arg ROM: +required {file_exists} "ROM file to analyze")
    )
    .get_matches();

    // Load the ROM.
    let rom_path = matches.value_of("ROM").unwrap();
    let rom = ROM::from(rom_path.into())?;

    // Run the command prompt.
    let mut app = App::new(rom);
    app.run();

    Ok(())
}