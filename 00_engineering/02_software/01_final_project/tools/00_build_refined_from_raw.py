import os
import matplotlib
import numpy
import shutil
import sqlite3
import sys
import re

IMAGE_LOG = "image_log.txt"
MOTORS_LOG = "motors_log.txt"

class App:

    def read_image_log(self, path):

        image_entries = dict()

        with open(path, 'r') as f:
            for line in f:

                toks = line.strip().split(" ")
                sequence = int(toks[0])
                timestamp = int(toks[1])
                frameid = int(toks[2])

                if sequence not in image_entries:
                    image_entries[sequence] = list()

                image_entries[sequence].append( (timestamp, frameid) )

        return image_entries

    def read_motors_log(self, path):

        motors_entries = dict()

        with open(path, 'r') as f:
            for line in f:

                toks = line.strip().split(" ")
                sequence = int(toks[0])
                timestamp = int(toks[1])
                steering = float(toks[2])

                if sequence not in motors_entries:
                    motors_entries[sequence] = list()

                motors_entries[sequence].append( (timestamp, steering) )

        return motors_entries

    def export_sequence(self, subdirectory, motors_entries, image_entries):

        print("Exporting a sequence from {}".format(subdirectory,))

        # interpolate steering values.

        samples = list()

        for timestamp, frameid in image_entries:
            a = [ i for i in range(len(motors_entries)-1) if motors_entries[i][0] <= timestamp and timestamp <= motors_entries[i+1][0] ]
            if a:
                i0 = a[0]
                i1 = a[-1]+1
                t0 = motors_entries[i0][0]
                t1 = motors_entries[i1][0]
                s0 = motors_entries[i0][1]
                s1 = motors_entries[i1][1]

                if 1 <= t1-t0 and t1-t0 <= 500:
                    steering = s0 + (s1-s0) * float(timestamp - t0) / float(t1 - t0)

                    samples.append( (timestamp, frameid, steering) )

        take_this_sequence = True
        take_this_sequence = take_this_sequence and ( len(samples) >= 100 )
        take_this_sequence = take_this_sequence and ( max([x[0] for x in samples]) > 7000 )

        if take_this_sequence:

            # create record in database

            description = "unnamed sequence"
            c = self.db.cursor()
            c.execute("INSERT INTO sequences(name) VALUES(?)", (description,))
            sequence_id = c.lastrowid

            # create sequence directory

            in_place = False

            if not in_place:
                dir_path = os.path.join(self.export_directory, str(sequence_id).zfill(9) )
                os.mkdir(dir_path)

            for timestamp, frameid, steering in samples:

                filename = str(frameid).zfill(12) + ".png"
                source_file = os.path.join(self.root_directory, subdirectory, filename)

                if in_place:
                    assert( os.path.isfile(source_file) )
                    recorded_path = source_file
                else:
                    dest_file = os.path.join(dir_path, filename)
                    shutil.copyfile(source_file, dest_file)
                    recorded_path = os.path.relpath(dest_file, start=self.export_directory)

                # create record in database

                c.execute("INSERT INTO samples(sequence_id, timestamp, image, steering, dataset_id) VALUES(?,?,?,?,?)", (sequence_id, timestamp, recorded_path, steering, self.default_dataset_id))

    def parse_subdirectory(self, subdirectory):

        motors_entries = self.read_motors_log(os.path.join(self.root_directory, subdirectory, MOTORS_LOG))
        image_entries = self.read_image_log(os.path.join(self.root_directory, subdirectory, IMAGE_LOG))

        sequences = set(motors_entries.keys()).intersection(image_entries.keys())
        for s in sequences:
            self.export_sequence(subdirectory, motors_entries[s], image_entries[s])

    def parse_root_directory(self):

        for fname in os.listdir(self.root_directory):
            if os.path.isdir(os.path.join(self.root_directory, fname)):
                self.parse_subdirectory(fname)

    def get_or_create_dataset(self, name):

        c = self.db.cursor()

        c.execute("SELECT id FROM datasets WHERE name=?", (name,))
        r = c.fetchone()
        if r:
            ret = r[0]
        else:
            c.execute("INSERT INTO datasets(name) VALUES(?)", (name,))
            ret = c.lastrowid

        return int(ret)

    def export(self, root_directory, export_directory):

        self.root_directory = root_directory
        self.export_directory = export_directory

        self.db = sqlite3.connect(os.path.join(export_directory, "db.sqlite"))
        self.db.execute("CREATE TABLE IF NOT EXISTS datasets(id INTEGER PRIMARY KEY, name TEXT)")
        self.db.execute("CREATE TABLE IF NOT EXISTS sequences(id INTEGER PRIMARY KEY, name TEXT)")
        self.db.execute("CREATE TABLE IF NOT EXISTS samples(id INTEGER PRIMARY KEY, sequence_id INTEGER, timestamp INTEGER, image TEXT, steering FLOAT, dataset_id INTEGER)")

        self.default_dataset_id = self.get_or_create_dataset("default")
        self.get_or_create_dataset("bad")
        self.get_or_create_dataset("train")
        self.get_or_create_dataset("test")

        self.parse_root_directory()

        self.db.commit()
        self.db.close()

if __name__ == '__main__':
    
    if len(sys.argv) != 3:
        print("Bad command line!")
        exit(1)

    root_directory = sys.argv[1]
    export_directory = sys.argv[2] #os.getcwd()

    app = App()
    app.export(root_directory, export_directory)

