from flask import Flask, render_template, flash, request
from wtforms import Form, TextField, TextAreaField, validators, StringField, SubmitField
import WrapLem

# App config.
DEBUG = True
app = Flask(__name__)
app.config.from_object(__name__)
app.config['SECRET_KEY'] = '7d441f27d441f27567d441f2b6176a'

polem = None


class ReusableForm(Form):
    orths = TextField('Orths:', validators=[validators.required()], default="Lidze światowej")
    lemmas = TextField('Lemmas:', validators=[validators.required()], default="liga światowy")
    tags = TextField('Tags:', validators=[validators.required()], default="subst:sg:loc:f adj:sg:loc:f:pos")

    @app.before_first_request
    def before_first_request_func():
        global polem
        polem = WrapLem.CascadeLemmatizer.assembleLemmatizer()

    @app.route("/", methods=['POST'])
    def json():
        content = request.get_json()
        print(content)
        orths = content["orths"]
        lemmas = content["lemmas"]
        tags = content["tags"]
        phrase_lemma = polem.lemmatizeS(orths, lemmas, tags, False)
        return phrase_lemma

    @app.route("/form", methods=['GET', 'POST'])
    def hello():
        global polem

        form = ReusableForm(request.form)

        if request.method == 'POST':
            orths = request.form['orths']
            lemmas = request.form['lemmas']
            tags = request.form['tags']

            if form.validate():
                phrase_lemma = polem.lemmatizeS(orths, lemmas, tags, False)
                flash(phrase_lemma)
            else:
                flash('All the form fields are required.')

        return render_template('form.html', form=form, template="form-template")


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000)
