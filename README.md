# OceanRoute Nav

OceanRoute Nav is a C++ desktop prototype for exploring maritime shipping routes. It draws a port network over a world map and can compare routes by cost or travel time.

## Features

- Visual port and route map built with SFML.
- Cheapest and fastest route searches.
- Filters for shipping company and date.
- Multi-leg route editing and a ship queue model.
- Custom graph, linked-list, and queue structures implemented in C++.
- Port, port-charge, and route data loaded from text files.

## Build requirements

- A C++ compiler with C++11 support.
- SFML 2.x development libraries built for the same compiler.

The included `Makefile.win` was generated for a specific local Dev-C++ and SFML installation. Update its include and library paths for your machine, or configure those paths in your IDE.

A typical MinGW build from the project directory is:

```powershell
g++ main.cpp -std=c++11 -I<sfml-include> -L<sfml-lib> -o OceanRouteNav.exe -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

For a shared SFML build on Windows, make sure the matching SFML runtime DLLs are available beside the executable or on `PATH`.

## Run

Run the executable with the project directory as the working directory. The application loads these files by relative path:

- `world_map.jpeg`, `ocean_currents.jpeg`, `anchor.jpeg`
- `ports.txt`, `PortCharges.txt`, `Routes.txt`

Keeping the working directory at the repository root lets the application find those assets and data files.

## Project structure

- `main.cpp` — SFML interface, custom data structures, route search, and data loading.
- `ports.txt`, `PortCharges.txt`, `Routes.txt` — port and route data.
- `*.jpeg` — map and interface images.
- `Makefile.win` — original Windows/MinGW build configuration; update local paths before use.
