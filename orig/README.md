Place your copy of `Tornado.exe` (version 1.0.0.67, SHA-1 `20bf7200effe49d2dff295a3b6c546a5804e06bc`) in this folder, then run (on Windows):

```bat
py tools\check_target.py
```

On Linux or macOS use `python3 tools/check_target.py`.

The target is the complete image extracted from the installer in the game's ISO (the `Tornado-WT.exe` payload). It differs from the earlier retail image `71be8eb9…` in 18 bytes at VA `0x59188f`, in `lib/ode/joint`; see `config/target.json`.

Everything in this folder except this README is git-ignored. Never commit game files.
