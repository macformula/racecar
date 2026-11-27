{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "https://channels.nixos.org/nixpkgs-unstable/nixexprs.tar.zst";
  };

  outputs = inputs:
    let
      forAll = f: builtins.mapAttrs f inputs.nixpkgs.legacyPackages;
    in {
      devShells = forAll (system: pkgs:
        let
          py = pkgs.python314.withPackages (ps: with ps; [
            numpy
            protobuf
            grpcio-tools
            cantools
          ]);
        in {
          default = pkgs.mkShell {
            packages = [
              pkgs.platformio-core
              py
              pkgs.pre-commit
              pkgs.clang-tools
            ];
          };
        });
    };
}
