#!/bin/zsh
# notarize
#
# Notarize the Developer-ID-signed bbpl executable produced by produce.sh.

set -e

BBPL='./build/bbpl_SDK/bbpl'
ARCHIVE='./build/bbpl-notarize.zip'
PROFILE='bBpiano-L Notarization'
EXPECTED_TEAM_ID='L746W9JTR3'

echo
echo '== bBpiano-L Notarization =='
echo

# ------------------------------------------------------------
# 1. Check executable
# ------------------------------------------------------------

if [[ ! -f "$BBPL" ]]; then
    echo "Error: bbpl executable not found:"
    echo "  $BBPL"
    echo
    echo "Run produce.sh first."
    exit 1
fi

# ------------------------------------------------------------
# 2. Verify Developer ID signature
# ------------------------------------------------------------

echo '== Verify Developer ID Signature =='

codesign \
    --verify \
    --strict \
    --verbose=4 \
    "$BBPL"

TEAM_ID=$(
    codesign -dvv "$BBPL" 2>&1 \
        | sed -n 's/^TeamIdentifier=//p'
)

if [[ "$TEAM_ID" != "$EXPECTED_TEAM_ID" ]]; then
    echo
    echo "Error: unexpected TeamIdentifier."
    echo "Expected: $EXPECTED_TEAM_ID"
    echo "Actual:   $TEAM_ID"
    exit 1
fi

echo
echo "Developer ID Team: $TEAM_ID"
echo

# ------------------------------------------------------------
# 3. Create notarization archive
# ------------------------------------------------------------

echo '== Create Notarization Archive =='

rm -f "$ARCHIVE"

ditto \
    -c \
    -k \
    --keepParent \
    "$BBPL" \
    "$ARCHIVE"

echo "Created: $ARCHIVE"
echo

# ------------------------------------------------------------
# 4. Submit to Apple Notary Service
# ------------------------------------------------------------

echo '== Submit to Apple Notary Service =='
echo

xcrun notarytool submit \
    "$ARCHIVE" \
    --keychain-profile "$PROFILE" \
    --wait

# ------------------------------------------------------------
# 5. Final local signature verification
# ------------------------------------------------------------

echo
echo '== Final Signature Verification =='

codesign \
    --verify \
    --strict \
    --verbose=4 \
    "$BBPL"

echo
echo '== Signature Identity =='

codesign \
    -dvvv \
    "$BBPL"

# ------------------------------------------------------------
# 6. Cleanup
# ------------------------------------------------------------

rm -f "$ARCHIVE"

echo
echo '========================================'
echo ' bBpiano-L notarization completed.'
echo '========================================'
echo
