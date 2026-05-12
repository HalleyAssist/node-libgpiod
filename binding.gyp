{
    "targets": [
        {
            "target_name": "node-libgpiod",
            "conditions": [
                ["OS=='linux'", {
                    "sources": [
                        "src/main.cc",
                        "src/misc.cc",
                        "src/chip.cc",
                        "src/line.cc",
                        "src/gpiod_mock.cc"
                    ],
                    "include_dirs" : [
                        "<!(node -e \"require('nan')\")"
                    ],
                    "defines": [
                        "<!@(if uname -r | tr '[:upper:]' '[:lower:]' | grep -q microsoft; then echo NODE_LIBGPIOD_MOCK=1; fi)"
                    ],
                    "libraries" : [
                        "<!@(if uname -r | tr '[:upper:]' '[:lower:]' | grep -q microsoft; then :; else pkg-config --libs libgpiod 2>/dev/null || echo -lgpiod; fi)"
                    ],
                    "cflags": [
                        "-O3"
                    ],
                    "lflags": [
                        "-flto"
                    ]
                }]
            ]
        }
    ],
}
