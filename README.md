<p align="center">
  <img src="assets/logo.jpg" alt="Logo" width="60%">
</p>

![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/giovanni-iannaccone/pwnit/cmake-single-platform.yml)
![License](https://img.shields.io/github/license/giovanni-iannaccone/pwnit)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/giovanni-iannaccone/pwnit)
![Number of GitHub contributors](https://img.shields.io/github/contributors/giovanni-iannaccone/pwnit)
[![Number of GitHub stars](https://img.shields.io/github/stars/giovanni-iannaccone/pwnit)](https://github.com/giovanni-iannaccone/pwnit/stargazers)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)

# ⚡pwnit
pwnit is a C++ tool built to automate the boring parts of binary exploitation. You give it a binary, a libc, or even a container. It helps you inspect the target, prepare the challenge environment, find useful gadgets, trace function dependencies, and get to the actual exploitation faster. Because spending ten minutes looking for `libc.so.6` inside a Docker image is not what anyone signed up for.

## 🔍 Inspect binaries
Get the information you usually gather with a handful of different tools, in one place.
```sh
pwnit check ./chall
```

It can inspect things such as:
- Security mitigations
- ELF sections
- Symbols
- Imported and exported functions
- Other useful ELF metadata

Informations can also be exported as JSON, making the output easy to consume from scripts or other tools.
```sh
pwnit check ./chall --symbols --json
```

## 🐳 Extract libc from containers
Sometimes a challenge gives you a Docker or Podman container instead of the libc. Naturally, pwnit can deal with that.
```sh
pwnit container <container-id> <port> --docker
```

It can inspect container environments and extract the relevant libc.so.6 and loader, saving you from manually digging through the filesystem. Docker and Podman are supported. Because yes, sometimes the challenge author really does give you a container and say "have fun".

## 🛠️ Prepare a challenge
Start pwnit and let it handle the setup.
```sh
pwnit start
```

The setup process can:
- Recognize binaries
- Obtain the appropriate loader
- Unstrip the libc
- Patch the binary to use the provided libc and loader
- Prepare the local execution environment
- Generate a solve template (template file in `~/.config/pwnit/templates/`)

If the binary recognition goes wrong, you can always specify them 
```sh
pwnit start chall libc.so.6 ld-linux.so
```

## 🔭 Find ROP gadgets
Search for ROP gadgets quickly without having to juggle several commands and tools.
```sh
pwnit rop chall
```

You can also search for specific gadgets.
```sh
pwnit rop chall --search "pop rax"
```

Useful when you already know what you want and don't want to wait around for a gadget finder to finish doing something that should have taken a moment.

# Roadmap
- [ ] Implement xrefs analysis
- [ ] Add rop gadgets export in json
- [ ] Support for multiple templates

## ⚖️ License
This project is licensed under the GPL-3.0 License. See the LICENSE file for details.

## ⚔️ Contact
- For any inquiries or support, please contact <a href="mailto:iannacconegiovanni444@gmail.com"> iannacconegiovanni444@gmail.com </a>.
- Visit my site for more informations about me and my work <a href="https://giovanni-iannaccone.github.io" target=”_blank” rel="noopener noreferrer"> https://giovanni-iannaccone.github.io </a>
