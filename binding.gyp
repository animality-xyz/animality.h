{
  "targets": [
    {
      "target_name": "animality_node",
      "sources": [ "node_addon.cpp" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS", "_AN_NODE_ADDON" ],
      "libraries": [ "<(module_root_dir)/deps/libanimal.a", "<(module_root_dir)/deps/libjson.a", "-lcurl" ]
    }
  ]
}