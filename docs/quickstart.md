# Quickstart

## 1) Verify environment

```bash
python3 scripts/check_env.py --sdk-dir sdk
```

## 2) Create app

```bash
python3 scripts/new_app.py --name "My App" --id myapp
```

## 3) Build standalone bundle

```bash
make APP_ID=myapp build
```

Bundle is ready in `dist/myapp.app/`.

If your project has `examples/myapp/resources/`, files are copied to
`dist/myapp.app/resources/`.

Done: you can now distribute `dist/myapp.app/` directly.
