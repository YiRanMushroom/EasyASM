# EasyASM - PicoBlaze Assembler

**EasyASM** is a general-purpose assembler and linker framework, currently focused on full support for the **Xilinx PicoBlaze** soft-core processor. It aims to be a modern replacement for the legacy DOS-based assembler, with improved usability, error diagnostics, and extensibility.

## 🔧 Usage

To assemble a PicoBlaze `.psm` source file and generate a `.mem` output:

```bash
EasyASM -l PicoBlaze -i path/to/source.psm -o path/to/output/dir
```

## 🧾 Command Line Options

| Option                       | Description                                                                 |
|-----------------------------|-----------------------------------------------------------------------------|
| `-h`, `--help`              | Display help information                                                    |
| `-l`, `--language-root-dir` | Path to the language definition directory — use `PicoBlaze`                 |
| `-i`, `--input`             | Path to the input source file (`.psm`)                                      |
| `-o`, `--output`            | Path to the output directory (optional, defaults to input file's directory) |

## 🧪 Example

```bash
EasyASM -l PicoBlaze -i examples/blink.psm -o out/
```

This compiles `blink.psm` using the PicoBlaze module and generates `blink.mem` in the `out/` folder.

## 📘 Notes

- The current implementation only supports the `PicoBlaze` language. Use `-l PicoBlaze` to specify it.
- The output filename is automatically derived from the input file name. For example, `foo.psm` produces `foo.mem`.
- Output will be written to the directory specified by `-o`, or the input file's directory if not given.

## 💡 About

EasyASM is designed to be modular and extensible. Most frontend logic is implemented in Lua, making it easy to patch or extend behavior, while the backend logic is in C++ for performance and control.