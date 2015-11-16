{
  "targets" : [
    {
      "target_name" : "gc-listener",
      "sources" : [ "gc-listener.cc" ]
    }
  ],
  "include_dirs" : [
    "<!(node -e \"require('nan')\")"
  ]
}
