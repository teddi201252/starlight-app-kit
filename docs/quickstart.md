# Quickstart

## 1) Verifica ambiente

```bash
python3 scripts/check_env.py --sdk-dir sdk
```

## 2) Crea app

```bash
python3 scripts/new_app.py --name "My App" --id myapp
```

## 3) Build standalone bundle

```bash
make APP_ID=myapp build
```

Bundle pronto in `dist/myapp.app/`.

Fine: ora puoi distribuire direttamente `dist/myapp.app/`.
