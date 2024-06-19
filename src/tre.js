let Module = require('./trejs.js');

window.Module = Module;

// placeholder untill findall webasm is loaded.
let findall_ = null;

Module().then(module => {
    findall_  = module.cwrap("findall", "string", ["string", "string"])
});


// Finds all occurrences of pattern in text, and returns a list of hierarchies for all the capture groups.
// Assumes capture groups never partly overlap, they always entirely encapsulated or not at all.
// A regex should adhere to this and otherwise, a hierarchy wouldn't be possible.
//
// `pattern` can be a tre-extension of a regular expression, but does not support named capture groups.
// TODO: test on unicode
function findall(pattern, text) {
    if (!findall_) {
        throw Error("tre approximate regex not initialized yet");
    }
    var matches = [];
    // remainder, i.e. text[start:], where matches may still exist.
    var start = 0;

    // run the code until no new matches
    while (true) {
        // run / parse / handle errors from tre
        var rawtext = findall_(pattern, text.slice(start));
        var result = JSON.parse(rawtext);
        if ('error' in result) throw new Error(result.error)

        // stop if no new matches.
        if (result.length <= 0)
            break;

        // tre didn't see the first start chars, so add those to the positional indices of `text`.
        for(var match of result) { match.start += start; match.end += start; match.children = []}

        // assume the first match as the entire match
        matches.push(result[0]);
        var current = result[0];

        // determine the parent for each of the other matches as per the hierarchy of capture groups.
        for(var match of result.slice(1)) {
            // if the previous match includes the current match, that should be the parent, i.e.
            // we are dealing with a capture group inside this previous capture group.
            var previous = current.children[current.children.length-1];
            if (previous && match.start >= previous.start && match.end <= previous.end) {
                current = previous;
            }
            // bubble up as long as the match doesn't fit the current 'parent'
            while (current.start > match.start || current.end < match.end) {
                current = current.parent;
            }
            match.parent = current;
            current.children.push(match);
        }

        // remove superfluous reference to parent.
        for(var match of result) { delete match.parent; }

        start += result[0].end; // update processed prefix of text
    }

    return matches;
}

module.exports = findall;
// for debugging, expose globally.
window.findall = findall;
