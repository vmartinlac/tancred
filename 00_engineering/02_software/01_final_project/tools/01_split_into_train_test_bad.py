import sqlite3
import random
import matplotlib
import matplotlib.pyplot
import numpy
import cv2
import sys
import os.path

if __name__ == '__main__':
    
    if len(sys.argv) != 2:
        print("Bad command line!")
        exit(1)

    root_directory = sys.argv[1]

    db = sqlite3.connect(os.path.join(root_directory, "db.sqlite"))

    c = db.cursor()
    c.execute("SELECT id FROM sequences")
    sequence_ids = [ int(row[0]) for row in c ]

    c.execute("SELECT id,name FROM datasets")
    dataset_ids = dict()
    for row in c:
        dataset_ids[row[1]] = int(row[0])

    assert( 'train' in dataset_ids and 'test' in dataset_ids and 'bad' in dataset_ids )
    
    for sequence_id in sequence_ids:

        c.execute("SELECT COUNT(id) FROM samples WHERE sequence_id=? AND dataset_id=?", (sequence_id, dataset_ids["default"]))
        if int(c.fetchone()[0]) == 0:
            continue

        c.execute("SELECT image FROM samples WHERE sequence_id=?", (sequence_id,))
        samples = c.fetchall()

        for sample in samples:
            im = cv2.imread( os.path.join(root_directory, sample[0]) )
            cv2.imshow("", im)
            cv2.waitKey(10)

        print("(g)ood, (b)ad or (s)kip?")
        go_on = True
        while go_on:
            r = input()
            go_on = r not in "gbs"

        if r == 'g':

            print("{} => good".format(sequence_id))

            c.execute("SELECT id FROM samples WHERE sequence_id=?", (sequence_id,))
            sample_ids = [ int(row[0]) for row in c ]

            X = numpy.random.random(size=len(sample_ids))

            labels = [ (dataset_ids['train'] if x < 0.8 else dataset_ids['test']) for x in X ]

            c.executemany("UPDATE samples SET dataset_id=? WHERE id=?", zip(labels, sample_ids))

        elif r == 'b':

            print("{} => bad".format(sequence_id))
            c.execute("UPDATE samples SET dataset_id=? WHERE sequence_id=?", (dataset_ids["bad"], sequence_id))

        else:

            print("Skipping {}".format(sequence_id))

        db.commit()

    db.close()

