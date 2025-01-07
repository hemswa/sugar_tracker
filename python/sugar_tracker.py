import csv
import datetime
import matplotlib.pyplot as plt

# Structure to hold sugar details
class SugarReading:
    def __init__(self, date, time, sugar_level, is_fasting):
        self.date = date
        self.time = time
        self.sugar_level = sugar_level
        self.is_fasting = is_fasting

# Function to get the current date and time
def get_current_date_time():
    now = datetime.datetime.now()
    date = now.strftime("%Y-%m-%d")
    time = now.strftime("%H:%M:%S")
    return date, time

# Function to save readings to a file
def save_reading(reading, filename):
    with open(filename, mode='a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([reading.date, reading.time, reading.sugar_level, int(reading.is_fasting)])

# Function to load readings from a file
def load_readings(filename):
    readings = []
    try:
        with open(filename, mode='r') as file:
            reader = csv.reader(file)
            for row in reader:
                date, time, sugar_level, is_fasting = row
                readings.append(SugarReading(date, time, int(sugar_level), bool(int(is_fasting))))
    except FileNotFoundError:
        pass
    return readings

# Function to generate graphs using matplotlib
def generate_graph(readings):
    fasting_indices = []
    non_fasting_indices = []
    fasting_levels = []
    non_fasting_levels = []

    for i, reading in enumerate(readings):
        if reading.is_fasting:
            fasting_indices.append(i)
            fasting_levels.append(reading.sugar_level)
        else:
            non_fasting_indices.append(i)
            non_fasting_levels.append(reading.sugar_level)

    plt.figure()
    if fasting_indices:
        plt.plot(fasting_indices, fasting_levels, 'b-o', label='Fasting')
    if non_fasting_indices:
        plt.plot(non_fasting_indices, non_fasting_levels, 'r-o', label='Non-Fasting')
    plt.legend()
    plt.title('Blood Sugar Readings')
    plt.xlabel('Reading Index')
    plt.ylabel('Sugar Level')
    plt.show()

# Main program
def main():
    filename = "sugar_readings.csv" # add path and file name to store sugar reading
    readings = []

    while True:
        print("\nBlood Sugar Tracker")
        print("1. Add Sugar Reading")
        print("2. Generate Graph")
        print("3. Exit")
        choice = input("Enter your choice: ")

        if choice == '1':
            date, time = get_current_date_time()
            sugar_level = int(input("Enter sugar level: "))
            fasting_input = input("Fasting (yes/no)? ").strip().lower()
            is_fasting = fasting_input in ['yes', 'y']

            reading = SugarReading(date, time, sugar_level, is_fasting)
            readings.append(reading)
            save_reading(reading, filename)

            print("Reading added successfully!")

        elif choice == '2':
            readings = load_readings(filename)
            if not readings:
                print("No readings available to generate a graph.")
            else:
                generate_graph(readings)

        elif choice == '3':
            break

        else:
            print("Invalid choice. Please try again.")

if __name__ == "__main__":
    main()
