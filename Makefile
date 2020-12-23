default:
	pip install -r requirements.txt
	pip install .
	rm -rf pylp.egg-info

install-dev:
	pip install -r requirements_dev.txt
	pip install -e .

.PHONY: tests
tests:
	pytest -v --cov=pylp -s pylp
	flake8 pylp
