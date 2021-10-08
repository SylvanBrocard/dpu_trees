dpu_trees
==============

|      CI              | status |
|----------------------|--------|
| conda.recipe         | [![Conda Actions Status][actions-conda-badge]][actions-conda-link] |
| pip builds           | [![Pip Actions Status][actions-pip-badge]][actions-pip-link] |
| wheel                | [![Wheels Actions Status][actions-wheels-badge]][actions-wheels-link] |

A project built with [pybind11](https://github.com/pybind/pybind11) and scikit-build.

[actions-badge]:           https://github.com/SylvanBrocard/dpu_trees/workflows/Tests/badge.svg
[actions-conda-link]:      https://github.com/SylvanBrocard/dpu_trees/actions?query=workflow%3AConda
[actions-conda-badge]:     https://github.com/SylvanBrocard/dpu_trees/workflows/Conda/badge.svg
[actions-pip-link]:        https://github.com/SylvanBrocard/dpu_trees/actions?query=workflow%3APip
[actions-pip-badge]:       https://github.com/SylvanBrocard/dpu_trees/workflows/Pip/badge.svg
[actions-wheels-link]:     https://github.com/SylvanBrocard/dpu_trees/actions?query=workflow%3AWheels
[actions-wheels-badge]:    https://github.com/SylvanBrocard/dpu_trees/workflows/Wheels/badge.svg

Installation
------------

- clone this repository
- `pip install ./dpu_trees`

Development
-----------

- clone this repository
- `cd dpu_trees`
- `pre-commit install`

OR

- clone this repository
- open folder in VS Code
- start in Dev Container

Testing
-------

- clone this repository
- `nox`

Test call
---------

```python
import dpu_trees
dpu_trees.add(1, 2)
```

[`cibuildwheel`]:          https://cibuildwheel.readthedocs.io
