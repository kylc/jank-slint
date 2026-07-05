{
  description = "Dev environment for jank-slint.";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    jank.url = "git+https://github.com/jank-lang/jank?submodules=1";
    self.submodules = true;
  };

  outputs =
    inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
        "x86_64-darwin"
      ];
      perSystem =
        {
          lib,
          inputs',
          pkgs,
          ...
        }:
        {
          devShells.default =
            with pkgs;
            mkShell {
              packages = [
                # Build tools.
                babashka
                bubblewrap
                cmake
                pkg-config
                leiningen

                # Jank
                inputs'.jank.packages.default

                # Slint
                rustc
                cargo
                rust-jemalloc-sys
                libxkbcommon
                libGL
                fontconfig
              ];

              LD_LIBRARY_PATH = lib.strings.makeLibraryPath [
                fontconfig
                wayland
                libxkbcommon
                libGL

                libx11
                libxcursor
                libxi
                libxrandr
                vulkan-loader
              ];

              # SLINT_BACKEND = "winit-software";
            };
        };
    };
}
