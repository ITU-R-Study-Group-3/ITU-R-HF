

# Choose a file from outptu from ITURNoise()
filename = "A_1m0h-24620d-081303t.csv"
# Read int the CSV file
x = csvread (filename);
# The first line is header so it will read in as 0s
# Trim it off and put and place the remainder in an array
y = x(2:end,5);
# Reshape array
yy = reshape(y, 361, 181);
# To get lat = y and long = x transpose the matrix
FaA = yy';
# Plot the contour
contour(FaA) 
