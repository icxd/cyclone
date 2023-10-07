struct String {
    data *rune
    len uint
}

fn String.c_str() !*c_char {
    let cstr := malloc(this.len + 1);
    for i in 0..this.len {
        let c rune = this.data[i];
        throw "non-ascii character in string" if c > 127;
        throw "negative character in string" if c < 0;
        cstr[i] = c as c_char;
    }
    cstr[this.len] = '\0';
    return cstr;
}

fn String.length() uint {
    return this.len;
}

fn String.at(index uint) !rune {
    throw "index out of bounds" if index >= this.len;
    return this.data[index];
}

fn main(argc uint, argv **c_char) !void {
    let str := "hi";
    for i in 0..str.length() {
        let c := try str.at(i);
        print("{}", c);
    }
    println();
}