This is just a temporary working directory for daq...

Generate Go DBC bindings:
```
go get -u go.einride.tech/can
cd dbc
go run go.einride.tech/can/cmd/cantool generate ./ ../generated/
```

Had to remove the "Reset" message from DashCommand to avoid name conflicts with bindings
> TODO: either find a better Go DBC binding generator or patch the upstream DBC to
> avoid having messages named "Reset" 