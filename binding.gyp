{
  "targets": [
    {
      "target_name": "animality_node",
      "sources": [ "node_addon.cpp" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "include_dirs": [ "<(module_root_dir)/deps" ],
      "libraries": [ "<(module_root_dir)/deps/libanimal.a", "-lcurl" ]
    }
  ]
}