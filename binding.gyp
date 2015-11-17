{
  "targets" : [
    {
      "target_name" : "gclistener",
      "sources" : [ "gc-listener.cc" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
