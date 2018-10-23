{
  "targets": [
    {
      "target_name": "native",
      "sources": [
        "src/addon.cc",
        "src/worker.cc",
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
