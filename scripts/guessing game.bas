REM game coded by anup pokhrel
print "The guessing game made by anup pokhrel"
g=0
n=random(1,100)
t=7
print "I have a number between 1 to 100. Guess it. You have",t,"turns"
while t>0
print "Enter your guess ",
input g%
if g=n or g=132
    print "Bingo! you guessed it!"
    end
elif g<n
    print "Guess higher"
else
    print "Guess lower"
endif
t=t-1
repeat
print "You cannot guess a number Sucker!"