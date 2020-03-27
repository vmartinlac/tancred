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
    c.execute("SELECT id FROM sequences WHERE dataset_id IN (SELECT id FROM datasets WHERE name='default')")
    sequence_ids = [ int(row[0]) for row in c ]

    c.execute("SELECT id,name FROM datasets")
    dataset_ids = dict()
    for row in c:
        dataset_ids[row[1]] = int(row[0])

    assert( 'train' in dataset_ids and 'test' in dataset_ids and 'bad' in dataset_ids )
    
    for sequence_id in sequence_ids:
        c.execute("SELECT timestamp, image, steering FROM samples WHERE sequence_id=?", (sequence_id,))
        samples = c.fetchall()

#        t = numpy.asarray([sample[0]*1.0e-3 for sample in samples])
#        s = numpy.asarray([sample[2]*180/numpy.pi for sample in samples])
#        matplotlib.pyplot.plot(t, s)
#        matplotlib.pyplot.ylim(-90.0, 90.0)
#        matplotlib.pyplot.show()
#        print(len(samples))

        for sample in samples:
            im = cv2.imread( os.path.join(root_directory, sample[1]) )
            cv2.imshow("", im)
            cv2.waitKey(15)

        #cv2.destroyAllWindows()

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

            c.executemany("UPDATE samples SET dataset_id=? WHERE id=?", zip(sample_ids, labels))

        elif r == 'b':

            print("{} => bad".format(sequence_id))
            c.execute("UPDATE samples SET dataset_id=? WHERE sequence_id=?)", (dataset_ids["bad"], sequence_id))

        else:

            print("Skipping {}".format(sequence_id))

        db.commit()

    db.close()

