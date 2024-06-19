# trejs

Javascript implementation of approximate regular expressions using webasm of the C-library tre.

It's a crude solution, feel free to improve.

## `src/libtre.so`

`libtre.so` Is a webasm-compiled shared object for `libtre`. You can probably use the one in this repo, as webasm should be cross-platform. You can, however, also create it by specifying `emcc` as the compiler and follow the normal [tre](https://github.com/laurikari/tre) build instructions. I.e.:

```bash
# clone git repo as needed
# git clone https://github.com/laurikari/tre
cd tre # move into repo

./utils/autogen.sh

CC=emcc ./configure
CC=emcc make
```

The object file should be in `lib/.libs/libre.so`.

## header files

If you want to rebuild, e.g. against a newer version of `tre`, make sure to include all header files in `local_includes` from the [tre git-repo](https://github.com/laurikari/tre).


## c++ code (`trejs.cpp`)

This is a wrapper around [tre](https://github.com/laurikari/tre) that outputs json, either with an error message `{"error": "some error"}`, and empty list `[]` if no matches or `[{"start": ..., "end": ..., "value": "..."}, ...]` with the hierarchy of the entire match and all the capture groups, as per [`tre_regexec` docs](https://laurikari.net/tre/documentation/regexec/).

This file is (also) alread build into webasm as `trejs.js` and `trejs.wasm`. However, you can (re)compile it as you wish as per:

```bash
MCC_DEBUG=1 emcc -O1 -sASSERTIONS=1 -s WASM=1 -s EXPORTED_RUNTIME_METHODS='["cwrap"]' -I . libtre.so trejs.cpp -o trejs.js
```

## wrapper js (`tre.js`)

This code returns a hierarchy of the capture groups, e.g.

```javascript
findall("(([0-9]{1,3})[a-z]\.){<2} ([0-9]{1,3} [0-9]{3}[A-Z]\.){<2}", "1zc. 234 3zAD.  1zc. 234 3zAD.")
```

Results in this hierarchy:

```javascript
[{
    "start":0,
    "end":14,
    "value":"1zc. 234 3zAD.",
    "children": [
        {"start":0,
            "end":4,
            "value":"1zc.",
            "children":[
            {"start":0,
                "end":2,
                "value":"1z"}
            ]},
            {"start":5,
            "end":14,
            "value":"234 3zAD.  1zc"}
    ]}, {
    "start":16,
    "end":30,
    "value":"1zc. 234 3zAD.",
    "children":[
        {"start":16,
            "end":20,
            "value":"1zc. 2",
            "children":[
            {"start":16,
                "end":18,
                "value":"1zc."}
            ]},
            {"start":21,
            "end":30,
            "value":"234 3zAD.",}
    ]
}]
```

## webpack / react:

The fallbacks for `fs`, `crypto` and `path` need to be disabled in `webpack.config.js`, to avoid errors (I'm usure why). Furthermore, a `.webasm` needs to be added for adding the webassembly as a (binary) asset. The latter doesn't seem to work for me, so I included `trejs.wasm` as a static file to the path in `module.exports.entry.output.path` of `webpack.config.js`.

```javascript
module.exports = {
    //...
    module: {
        rules: [
            //...
            {
                test: /\.wasm$/,
                type: "asset/inline",
            },
        ]
      },
  //...
  resolve: {
      ///...
      fallback: {
        fs: false,
        crypto: false,
        path: false
    },
  },
};
```

I added `trejs.js` and `tre.js` to a place in the webapp javascript source, from which I can import `findall`, e.g. in the `regex` folder:

```javascript
import findall from ".regex/tre";
```
