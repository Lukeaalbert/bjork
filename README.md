# Bjork
#### Bjork is a lightweight command line tool built to help users understand compilation and runtime errors. It's specifically intended for beginner programmers who, like I was, feel overwhelemed and confused by error messages. It's compatible across many programming languages.

Setup

```bash
curl -fsSL https://raw.githubusercontent.com/lukeaalbert/bjork/bjork-tools/bjork-install.sh | bash
```

To Use
```bash
bjork-listen g++ badcode.cpp
bjork --explain
```
```bash
bjork-listen ./badexecutible
bjork --explain
```
```bash
bjork --help
```

note: always run bjork-listen or else bjork will explain a previously captured error!

idea: "bjork --explain" to automatically clear the content from .last_error in order to better disambiguity of errors (in cases when users forget to run bjork-listen)