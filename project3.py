import pandas as pd
from matplotlib import pyplot as plt
import numpy as np


training_df = pd.read_csv("test.csv")
print(training_df.head(10))
print(training_df.describe())

price_data = training_df["Price"]
plt.hist(price_data,bins=20,edgecolor="Black")
plt.xlabel("Price")
plt.ylabel("no of houses")
plt.show()

cols = ["GrLivArea","BedroomAbvGr","TotalBsmtSF","FullBath"]
pd.plotting.scatter_matrix(training_df[cols], alpha=0.8, figsize=(12, 12), diagonal='hist')
plt.show()

weights = np.random.rand(25)

def pred(feature,weights):
    return np.dot(feature,weights)

prediction = []

for i, row in training_df.iterrows():
    features = row.values[1:26]
    Pred = pred(features,weights)
    prediction.append(float(Pred))

training_df["Predicted Price"] = prediction
print(training_df[["Price", "Predicted Price"]])


PriceActual = training_df["Price"].to_numpy()
PredPrice = training_df["Predicted Price"].to_numpy()


def loss(predprice, priceactual):
    MSE = (predprice - priceactual)**2
    return np.mean(MSE)

# mse = loss(PredPrice,PriceActual)
# print(f"MSE:{mse}")

def gradient(predprice, priceactual,feature):
    XT = np.transpose(feature)
    grad = (2/len(priceactual))*np.dot(XT,(predprice - priceactual))
    return grad


features_array = training_df.iloc[:,1:26].values
grad = gradient(PredPrice,PriceActual,features_array)
grad_magnitude = np.linalg.norm(grad)
#print(f"grad:{grad_magnitude}")


def update(weight,a,grad_magnitude):
    W = weight - a*grad_magnitude
    return W

# pred_test = pred(features_array, weights)
# MSE_test = loss(pred_test, training_df["Price"].to_numpy())
# print(f"MSE on the test set: {MSE_test}")


a1 = 10e-11
a2 = 10e-12
iterations = 500
iterations_list = []
MSE_list1 = []
MSE_list2 = []
weights1 = weights.copy()
weights2 = weights.copy()

for iteration in range(iterations):
    pred1 = pred(features_array,weights1)
    loss1 = loss(pred1,PriceActual)
    grad1 = gradient(pred1,PriceActual,features_array)
    weights1 = update(weights1,a1,grad1)

    pred2 = pred(features_array, weights2)
    loss2 = loss(pred2, PriceActual)
    grad2 = gradient(pred2, PriceActual, features_array)
    weights2 = update(weights2, a2, grad2)

    iterations_list.append(iteration)
    MSE_list1.append(loss1)
    MSE_list2.append(loss2)
    # if iterations % 10 == 0:
    #     print(f"Iterations {iteration}, Loss: {the_loss} ")

plt.plot(iterations_list,MSE_list1,label="a = 10e-11")
plt.plot(iterations_list,MSE_list2,label="a = 10e-12")
plt.legend()
plt.xlabel("Iterations")
plt.ylabel("MSE")
plt.show()

