{
  "targets": [
    {
      "target_name": "mp4thumb",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "src/mp4thumb.cpp" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/usr/local/include"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "conditions": [
        [ "OS=='linux'", {
          "link_settings": {
            "libraries": [
              "/usr/local/lib/libavformat.a",
              "/usr/local/lib/libavcodec.a",
              "/usr/local/lib/libswscale.a",
              "/usr/local/lib/libavutil.a",
              "-lz",
              "-lbz2",
              "-llzma",
              "-lpthread",
              "-lm",
              "-ldl"
            ]
          }
        }]
      ]
    }
  ]
}