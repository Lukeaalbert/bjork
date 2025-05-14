# Bjork
#### Bjork is a lightweight command line tool built to help users understand compilation and runtime errors across languages. It's specifically built for beginner programmers who, like we did, feel overwhelemed and confused by error messages.

 note: to test (for the time being, before we release) set gemini api key with:
```bash
export GEMINI_API_KEY="key"
```

### Contributers
* lukeaalbert@gmail.com
* berman.tanner@gmail.com

### Setup

```bash
curl -fsSL https://raw.githubusercontent.com/Lukeaalbert/bjork/main/bjork-tools/bjork-install.sh | bash
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
bjork --explanation-complexity 6
```
```bash
bjork --help
```

note: always run bjork-listen or else bjork will explain a previously captured error!

### TODO
(mark with "ip" for in progress or "x" for complete and email. ie,
"[ip, lukeaalbert@gmail.com] Do thing")
* [ ] Parse just the text (ai response) to print from JSON API response.
* [ ] Add "explanation complexity" prompt in bjork-install.sh and store set complexity on a file locally to use
* [ ] Add "bjork --explanation-complexity [1-10]" functionality to bjork.cpp. should override current set explanation complexity.
* [ ] Refactor bjork-install.sh and make it more professional and simple. ie, do we really need ALL of Libcurl? Super ideally, should be able to install and setup with "sudo apt install bjork".
* [ ] Decide on what we want to do with "last_error" file after after bjork.cpp is ran. Some ideas: (1) automatically clear it unless some flag "such as --no-clear" is also set. This is so a user always remebers to run "bjork-listen" before "bjork --explain". (2) don't automatically clear? but store last "bjork --explain" error used and make sure that contents of "last_error" isn't the same of that in the last stored error used?
* [] More error checks in bjork.cpp. Don't proceed if last_error is empty or doesn't contain a valid error. Brainstorm more for edge cases. 
* [] Design decisions regarding LLM to use. ie, should user put in their own API key in installation? should we allow them to use our API key? if so, how do we securly store that? Is there a good AI API to use that doesn't use API keys?
* [] Start on AI system prompt.
* [] Loading UI in terminal for while we are waiting for the API to return. probably should use some sort of threading here to be professional.
* [] Make more secure to protect against prompt injections.

### Ideas

*  "bjork-listen --append" to append on to .last_error (in some sort of well formatted/easy to send to LLM way)