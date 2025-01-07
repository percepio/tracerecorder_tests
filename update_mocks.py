# import required module
import os
import shutil

print('Mocks updating...')

# assign directory
source_directory = 'unit_test_mocks'
target_directory = 'mocks'

# iterate over files in
# that directory
for root, dirs, files in os.walk('.'):
    found = root.find('\\mocks')
    if found > 0:
        mocks_root = root[:found+6]
        for filename in files:
            if len(root) > found+7:
                target
                mock = os.path.join(root[found+7:], filename)
            else:
                mock = filename
            target = os.path.join(mocks_root, mock)
            source = os.path.join(source_directory, mock)

            target_ct = round(os.stat(target).st_mtime)
            source_ct = round(os.stat(source).st_mtime)
            if target_ct == source_ct:
                continue

            if target_ct > source_ct:
                print('File: '+target)
                print('WARNING! Target file newer than source file! Overwrite? (Y/N)')
                yn = input()
                if (yn.lower() != 'y'):
                    print('Skipping overwrite!')
                    continue
            
            print('Updating: '+target)
            shutil.copy2(source, target)

print('Done!')
