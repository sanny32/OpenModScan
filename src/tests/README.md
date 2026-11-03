# Regression tests

The suite uses Qt Test and CTest, supports Qt 5.15 and Qt 6, and does not
require a Modbus device. Qt Test is required when `BUILD_TESTING=ON` (the
default); use `-DBUILD_TESTING=OFF` for an application-only build.

From the repository root:

```sh
cmake -S src -B build-tests -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build-tests --target check-build --parallel
ctest --test-dir build-tests --output-on-failure --no-tests=error
```

Set `CMAKE_PREFIX_PATH` to your Qt installation if CMake cannot find it.
Use `-DUSE_QT5=ON` or `-DUSE_QT6=ON` to select a major version.
`check-build` builds all test executables without building the GUI or
updating translations. Run an individual suite with `ctest -R tst_core`,
or an individual Qt Test function with `./build-tests/tst_core frames`.

## Coverage by behavior

| Suite | Behaviors |
| --- | --- |
| `tst_modbustcpclient` | Wire encoding and response decoding for reads (01–04), single and multiple writes (05, 06, 0F, 10), application-level mask write (16) with address-base conversion, request group and device IDs, custom functions, fragmented TCP frames, protocol exceptions, timeout/retry and disconnected requests; TCP scanner discovery, device/gateway exceptions, silent devices, progress and cancellation |
| `tst_modbusrtutraffic` | RTU over TCP and optional serial RTU: complete/fragmented frames, exceptions, invalid and incomplete CRC, receive-traffic signals |
| `tst_core` | Signed/unsigned 16/32/64-bit and floating-point register decoding, byte/word order, insufficient data, address-space boundaries in both address bases, unread versus zero register values, fixed RTU/TCP wire fixtures, corruption/truncation, INI/XML window persistence and normalization |
| `tst_datasimulator` | Increment/decrement and wraparound, coil toggling, random bounds, multiregister reservations, device/area isolation, pause/resume/stop |
| `tst_mbmap` | Device-map parsing, grouping and placement, read limits, write-only barriers, descriptions, address bases and library cache paths |

The test server binds only to loopback on an automatically assigned port.
Network tests need permission to create local sockets.
Fixed byte fixtures provide expected wire values independently of the
production encoders. Timer tests wait for observable events with a deadline;
random tests assert bounds rather than a particular random sequence.
INI tests use a temporary directory and do not modify application settings.

## Serial RTU without hardware

By default the serial RTU rows are explicitly skipped. To enable them on Linux,
install `socat` and keep a virtual serial pair running:

```sh
socat pty,raw,echo=0,link=/tmp/omodscan-client pty,raw,echo=0,link=/tmp/omodscan-peer
```

In a second terminal:

```sh
OMODSCAN_TEST_SERIAL_CLIENT=/tmp/omodscan-client \
OMODSCAN_TEST_SERIAL_PEER=/tmp/omodscan-peer \
ctest --test-dir build-tests --output-on-failure --no-tests=error
```

The GitHub Actions `Tests` workflow runs both Qt versions on Linux and enables
these serial cases with `socat`.

## Scope

This is a behavioral regression baseline for the 2.0 work, not a statement of
100% line coverage. Automated GUI interaction, printing, serial scanner orchestration,
physical serial hardware, and every extended message-parser function remain
outside this suite. The application code is unchanged.
