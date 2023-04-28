[![CMake](https://github.com/kuvaus/llama-chat/actions/workflows/cmake.yml/badge.svg)](https://github.com/kuvaus/llama-chat/actions/workflows/cmake.yml)
# Llama-Chat
Simple command line chat program for [LLaMA](https://en.wikipedia.org/wiki/LLaMA) models written in C++. Based on [llama.cpp](https://github.com/ggerganov/llama.cpp) with some bindings from [gpt4all-chat](https://github.com/nomic-ai/gpt4all-chat).

<img alt="Llama-Chat demo" src="https://user-images.githubusercontent.com/22169537/234532183-eb70ebca-7136-43d6-ac07-4ea9b4fab28f.gif" width="600" />

# Table of contents
<!-- TOC -->
* [LLaMA-model](#llama-model)
* [Installation](#installation)
* [Usage](#usage)
* [Detailed command list](#detailed-command-list)
* [License](#license)
<!-- TOC -->

## LLaMA model
You need to download a LLaMA model first. The original weights are for research purposes and you can apply for access [here](https://ai.facebook.com/blog/large-language-model-llama-meta-ai/). Below are direct links to derived models:a

>- Vicuna 7b **v1.1**: [ggml-vicuna-7b-1.1-q4_2.bin](https://gpt4all.io/models/ggml-vicuna-7b-1.1-q4_2.bin)
>- Vicuna 13b **v1.1**: [ggml-vicuna-13b-1.1-q4_2.bin](https://gpt4all.io/models/ggml-vicuna-13b-1.1-q4_2.bin)
>- GPT-4-All **l13b-snoozy**: [ggml-gpt4all-l13b-snoozy.bin](https://gpt4all.io/models/ggml-gpt4all-l13b-snoozy.bin)

The LLaMA models are quite large: the 7B parameter versions are around 4.2 Gb and 13B parameter 8.2 Gb each. The chat program stores the model in RAM on runtime so you need enough memory to run. You can get more details on LLaMA models from the [whitepaper](https://arxiv.org/abs/2302.13971) or META AI [website](https://ai.facebook.com/blog/large-language-model-llama-meta-ai/).

## Installation
### Download

```sh
git clone --recurse-submodules https://github.com/kuvaus/llama-chat
cd llama-chat
```

### Build

```sh
mkdir build
cd build
cmake ..
cmake --build . --parallel
```
> **Note**
> If you have an old processor, you can turn AVX2 instructions off in the build step with `-DAVX2=OFF` flag 

## Usage

After compiling, the binary is located at:

```sh
build/bin/chat
```
But you're free to move it anywhere. Simple command for 4 threads to get started:
```sh
./chat -m "/path/to/modelfile/ggml-vicuna-13b-1.1-q4_2.bin" -t 4
```

Happy chatting!

## Detailed command list
You can view the help and full parameter list with:
`
./chat -h
`

```sh
usage: ./bin/chat [options]

A simple chat program for LLaMA based models.
You can set specific initial prompt with the -p flag.
Runs default in interactive and continuous mode.
Type 'quit', 'exit' or, 'Ctrl+C' to quit.

options:
  -h, --help            show this help message and exit
  --run-once            disable continuous mode
  --no-interactive      disable interactive mode altogether (uses given prompt only)
  -s SEED, --seed SEED  RNG seed (default: -1)
  -t N, --threads N     number of threads to use during computation (default: 4)
  -p PROMPT, --prompt PROMPT
                        prompt to start generation with (default: empty)
  --random-prompt       start with a randomized prompt.
  -n N, --n_predict N   number of tokens to predict (default: 200)
  --top_k N             top-k sampling (default: 50400)
  --top_p N             top-p sampling (default: 1.0)
  --temp N              temperature (default: 0.9)
  -b N, --batch_size N  batch size for prompt processing (default: 9)
  -r N, --remember N    number of chars to remember from start of previous answer (default: 200)
  -j,   --load_json FNAME
                        load options instead from json at FNAME (default: empty/no)
  -m FNAME, --model FNAME
                        model path (current: models/ggml-vicuna-13b-1.1-q4_2.bin)
```

You can also fetch parameters from a json file with `--load_json "/path/to/file.json"` flag.  The json file has to be in following format:

```javascript
{"top_p": 1.0, "top_k": 50400, "temp": 0.9, "n_batch": 9}
```
This is useful when you want to store different temperature and sampling settings.

## License

This project is licensed under the MIT [License](https://github.com/kuvaus/llama-chat/blob/main/LICENSE)
