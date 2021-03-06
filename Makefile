.PHONY: archive beta copyright test
SHELL=/bin/bash

archive:
	xcodebuild -scheme Awful archive

beta:
	Xcode/version-bump.rb beta

copyright:
	Xcode/fix-copyright.rb

minor:
	Xcode/version-bump.rb minor

test:
	set -o pipefail && xcodebuild -scheme Awful -configuration Release -sdk iphonesimulator test | xcpretty -c
