# route_planner

A road network routing engine modeled after [OSRM](https://github.com/Project-OSRM/osrm-backend) and built using educational resources provided by Dr. Hannah Bast in her course [Efficient Route Planning](http://ad-wiki.informatik.uni-freiburg.de/teaching/EfficientRoutePlanningSS2012).

Purely an academic exercise, not intended for production usage.

## Usage

### Demo

A live demo using vector tiles provided by [MapBox](https://www.mapbox.com/) and configured with a small graph containing a region within Burlington VT is available at:
<demo.keyanp.com/route_planner>

You can make direct requests to the demo routing server as well:
```
$ curl http://demo.keyanp.com/route?44.478080,-73.199450,44.487469,-73.220668
```

### Build from source

You will need a C++11 compatible compiler (`g++`) and `make`.

Install dependencies (Debian):
```
$ apt-get update && apt-get install -y g++ make
```

Install dependencies (MacOSX):
```
$ brew install make
# Follow prompts to install Xcode developer tools
$ g++
```

Download boost:
```
$ make install
```

Build and run benchmarks:
```
$ make install build && ./main
```

### Local server

Build and run in server mode:
```
$ make build && ./main [port]
```

### Development

Run tests with:
```
$ make test
```

A Debian VM is provided to allow for use of `gdb` and `valgrind` without running into issues with MacOSX:
```
# Run Debian VM and login
$ vagrant up && vagrant ssh

# Keep local dir in sync with VM (run from host machine)
$ vagrant rsync-auto
```
