import re
import requests
import hashlib

def check_password_strength(password):
    score = 0
    suggestions = []

    # Length check
    if len(password) >= 12:
        score += 2
    elif len(password) >= 8:
        score += 1
    else:
        suggestions.append("Increase length to at least 12 characters.")

    # Upper and lower case check
    if re.search(r'[A-Z]', password) and re.search(r'[a-z]', password):
        score += 2
    else:
        suggestions.append("Use both uppercase and lowercase letters.")

    # Digit check
    if re.search(r'\d', password):
        score += 2
    else:
        suggestions.append("Include at least one number.")

    # Special character check
    if re.search(r'[!@#$%^&*(),.?":{}|<>]', password):
        score += 2
    else:
        suggestions.append("Include at least one special character.")

    # Classify password strength
    if score >= 7:
        strength = "Strong"
    elif score >= 4:
        strength = "Moderate"
    else:
        strength = "Weak"

    return strength, suggestions

def check_pwned(password):
    """Check if a password has been leaked using Have I Been Pwned API."""
    sha1_hash = hashlib.sha1(password.encode()).hexdigest().upper()
    prefix, suffix = sha1_hash[:5], sha1_hash[5:]

    url = f"https://api.pwnedpasswords.com/range/{prefix}"
    response = requests.get(url)

    if suffix in response.text:
        return True
    return False

# Get user input
password = input("Enter your password: ")
strength, suggestions = check_password_strength(password)

print(f"Password Strength: {strength}")
if suggestions:
    print("Suggestions to improve:")
    for s in suggestions:
        print(f"- {s}")

if check_pwned(password):
    print("⚠️ Warning: This password has been found in data breaches. Choose a new one!")
else:
    print("✅ This password has not been found in known breaches.")
