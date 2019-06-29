#!/usr/bin/env python

import click

from .app import App


@click.command()
@click.argument("rom", type=click.Path(exists=True))
def run(rom: str):
    app = App(rom)
    app.cmdloop()


if __name__ == "__main__":
    run()
