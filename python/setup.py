import ez_setup
ez_setup.use_setuptools()
from setuptools import setup
from setuptools.command.install import install

####################################################################
#                    CONFIGURATION
####################################################################

# do the build/install
setup(
    name="sampling",
    version=0.1,
    description="Python package for importance sampling.",
    long_description="A Python wrapper to a C++ library implementing various \
    importance sampling algorithms.",
    author="Yannik Behr",
    author_email="yannikbehr@yanmail.de",
    url="",
    license="GPL v3",
    packages=['sampling'],
    package_data={"sampling": ["_sampling.so"]},
)
