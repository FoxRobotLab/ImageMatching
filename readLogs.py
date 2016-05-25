
class FeatureData(object):

    def __init__(self, type, maxLimit):
        self.minVal = maxLimit
        self.featureType = type
        self.maxVal = -1
        self.total = 0
        self.count = 0

    def nextValue(self, next):
        if next < self.minVal:
            self.minVal = next
        if next > self.maxVal:
            self.maxVal = next
        self.total += next
        self.count += 1

    def reportValues(self):
        avg = self.total / self.count
        return (self.featureType, self.minVal, self.maxVal, avt)

    def __str__(self):
        template = "Type: {0:s}; Min Value = {1:6.2f}; Max Value = {2:6.2f}; Average Value = {3:6.2f}"
        if self.count == 0:
            avg = 0.0
        else:
            avg = self.total / self.count
        s = template.format(self.featureType, self.minVal, self.maxVal, avg)
        return s

    


def minMaxAvgScores(filename):
    inFile = open("logs/" + filename, 'r')
    houghData = FeatureData("Hough Lines", 10000)
    segmentData = FeatureData("Segmentation", 10000)
    colorSigData = FeatureData("Color Signa", 10000)
    relNormData = FeatureData("Relative Norms", 10000)
    overallData = FeatureData("Overal Score", 10000)
    for line in inFile:
        if "Hough sim" in line:
            scoreNum = getScoreInLine(line)
            houghData.nextValue(scoreNum)
        elif "Segment sim" in line:
            scoreNum = getScoreInLine(line)
            segmentData.nextValue(scoreNum)
        elif "Relative Norm" in line:
            print(line)
            relNorm1 = getScoreInLine(line)
            line2 = inFile.readline()
            print("  ", line2)
            line2 = inFile.readline()
            print("     ", line2)
            relNorm2 = getScoreInLine(line2)
            diff = abs(relNorm1 - relNorm2)
            relNormData.nextValue(diff)
        elif "ColorSig sim" in line:
            scoreNum = getScoreInLine(line)
            colorSigData.nextValue(scoreNum)
        elif "Final similarity" in line:
            scoreNum = getScoreInLine(line)
            overallData.nextValue(scoreNum)
    inFile.close()
    print(houghData)
    print(segmentData)
    print(colorSigData)
    print(relNormData)
    print(overallData)


def getScoreInLine(line):
    words = line.split()
    score = words[-1]
    try:
        scoreNum = float(score)
    except:
        print("Score string", score, "could not be converted to a float")
        scoreNum = 0
    return scoreNum
    



minMaxAvgScores("FriJun13_16:38:00_2014.txt")
