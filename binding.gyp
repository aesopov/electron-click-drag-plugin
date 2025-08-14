{
  "targets": [
    {
      "target_name": "drag",
      "sources": [ "drag.cc" ],
      "include_dirs": [
        "<!(node -p \"require('node-addon-api').include_dir\")",
        "<!(node -p \"require('node-addon-api').include\")"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "conditions": [
        [ "OS=='win'", {
          "libraries": [ "user32.lib" ]
        }]
      ]
    }
  ]
}
