# jank-slint

[![Clojars Project](https://img.shields.io/clojars/v/io.github.kylc/jank-slint.svg)](https://clojars.org/io.github.kylc/jank-slint)

Bindings to the [Slint](https://github.com/slint-ui/slint) GUI toolkit for jank.

## Installation

Add the following dependency to your Leiningen project file:

``` clojure
[io.github.kylc/jank-slint "0.1.1"]
```

Ensure that you have all of the Slint dependencies installed (Rust/cargo toolchain, graphics libs, etc.)

NOTE: When building your project, you must invoke lein-jank with `--disable-sandbox` for now, as slint-sys downloads non-vendored Cargo dependencies.

## Usage

See the examples in the [examples](./examples) directory.

## Licensing

Ensure that you are in compliance with all licensing restrictions of the
[Slint](https://github.com/slint-ui/slint) project, which is licensed under the
Slint Royalty-free License, GNU GPLv3, or Slint Commercial license.
