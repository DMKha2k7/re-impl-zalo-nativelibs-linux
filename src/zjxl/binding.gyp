{
  "targets": [
    {
      "target_name": "jxl",
      "sources": [
        "src/addon.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/usr/include/opencv4"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "conditions": [
        ["OS=='linux'", {
          "cflags_cc": [
            "-std=c++17",
            "-fexceptions",
            "<!@(pkg-config --cflags libjxl libjxl_threads opencv4 libjpeg 2>/dev/null || echo '')"
          ],
          "libraries": [
            "<!@(pkg-config --libs libjxl libjxl_threads opencv4 libjpeg 2>/dev/null || echo '-ljxl -ljxl_threads -lopencv_core -lopencv_imgproc -ljpeg')"
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
