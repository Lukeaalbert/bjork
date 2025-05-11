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

ideas:

1) "bjork --explain" to automatically clear the content from .last_error in order to better disambiguity of errors (in cases when users forget to run bjork-listen)

2) "bjork-listen --append" to append on to .last_error (in some sort of well formatted/easy to send to LLM way)

3) set "explanation complexity" from 1-10 in bjork-install.sh and save in file locally on target machine. use experince level to cater the level of technicality in explanations. adjust this level later with "bjork --set-explanation-complexity <1-10>"