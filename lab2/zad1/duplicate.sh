#!/bin/bash

eval "printf \"$(cat "$1")%.0s\" {1..$3}" > "$2"