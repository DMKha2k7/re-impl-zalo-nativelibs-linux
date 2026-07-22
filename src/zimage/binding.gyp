{
  "targets": [
    {
      "target_name": "zimage",
      "sources": [
        "src/zimage.cc",
        "src/buffer_thumbnail_worker.cc",
        "src/file_thumbnail_worker.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<(module_root_dir)"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
      "conditions": [
        ["OS=='linux'", {
          "cflags_cc": [
            "-std=c++17",
            "-O2",
            "-fexceptions",
            "<!@(pkg-config --cflags vips-cpp vips)"
          ],
          "libraries": [
            "<!@(pkg-config --libs vips-cpp vips)"
          ],
          "ldflags": [
            "-Wl,-rpath,'$$ORIGIN/lib'",
            "-Wl,-rpath,'$$ORIGIN'",
            "-Wl,-z,origin",
            "-Wl,--disable-new-dtags"
          ]
        }]
      ]
    }
  ]
}

