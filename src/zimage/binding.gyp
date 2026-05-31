{
  "targets": [
    {
      "target_name": "zimage",
      "sources": ["src/addon.cpp"],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "conditions": [
        ["OS=='linux'", {
          "cflags_cc": [
            "-std=c++17", "-fexceptions",
            "<!@(pkg-config --cflags vips)"
          ],
          "libraries": [
            "<!@(pkg-config --libs vips)"
          ],
          "ldflags": [
            "-Wl,-rpath,'$$ORIGIN'",
            "-Wl,-rpath,'$$ORIGIN/../linux_x64'"
          ]
        }]
      ]
    }
  ]
}
